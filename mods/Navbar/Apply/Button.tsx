import Link from 'next/link'

let ApplyButton = () => {
  return <Link href="/apply">
    <a className="flex cursor-pointer space-x-2 items-center py-2 px-6 rounded-lg transition-colors bg-black text-gray-200" style={{width: 92}}>
      Apply
    </a>
  </Link>
}
export default ApplyButton