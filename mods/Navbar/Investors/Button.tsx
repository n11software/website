import Link from 'next/link'

let InvestorsButton = props => {
  return <Link href="/investors">
    <a className="flex cursor-pointer space-x-2 items-center py-2 px-2 rounded-lg transition-colors active:bg-black active:text-gray-200 text-gray-800 hover:bg-stone-300">
      Investors
    </a>
  </Link>
}
export default InvestorsButton